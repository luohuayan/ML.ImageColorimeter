#include "qtmessagelogwidget.h"
#include <QApplication>
#include <QClipboard>
#include <QHash>
#include <QPainter>
#include <QPaintEvent>
#include <cassert>
#include <algorithm>
#include <iterator>
#include <QToolBar>
#include <QTextStream>


QtMessageLogWidgetPrivate::QtMessageLogWidgetPrivate( QtMessageLogWidget * q ) :
    q_ptr( q ),
    historySize( 0xFFFFFFFF ),
    minimumVisibleLines( 1 ),
    minimumVisibleColumns( 1 ),
    alternatingRowColors( false )
{
    // PENDING(marc) find all the magic flags we need here...
    QWidget * const vp = q->viewport();
    vp->setBackgroundRole( QPalette::Base );
    vp->setAttribute( Qt::WA_StaticContents );
    vp->setAttribute( Qt::WA_NoSystemBackground );
#ifndef QT_NO_CURSOR
    vp->setCursor(Qt::IBeamCursor);
#endif
}

QtMessageLogWidgetPrivate::~QtMessageLogWidgetPrivate() {
}


void QtMessageLogWidgetPrivate::updateCache() const
{

    if ( cache.dirty >= Cache::FontMetrics ) {
        const QFontMetrics & fm = q_ptr->fontMetrics();
        cache.fontMetrics.lineSpacing = fm.lineSpacing();
        cache.fontMetrics.ascent = fm.ascent();
#if QT_VERSION < 0x040200
        cache.fontMetrics.averageCharWidth = fm.width( QLatin1Char( 'M' ) );
#else
        cache.fontMetrics.averageCharWidth = fm.averageCharWidth();
#endif

        QVector<int> & lw = cache.fontMetrics.lineWidths;
        lw.clear();
        lw.reserve( lines.size() );
        for (auto it = lines.begin(); it != lines.end(); ++it) {
            lw.push_back( fm.width( it->text ));
        }
    }

    if ( cache.dirty >= Cache::Dimensions ) {
        const QVector<int> & lw = cache.fontMetrics.lineWidths;
        const QVector<int>::const_iterator it =
            std::max_element( lw.begin(), lw.end() );
        if ( it == lw.end() ) {
            cache.dimensions.indexOfLongestLine = -1;
            cache.dimensions.longestLineLength = 0;
        } else {
            cache.dimensions.indexOfLongestLine = it - lw.begin();
            cache.dimensions.longestLineLength = *it;
        }
    }

    cache.dirty = false;
}

void QtMessageLogWidgetPrivate::enforceHistorySize()
{
    const size_t numLimes = lines.size();
    if ( numLimes <= historySize )
        return;
    const int remove = numLimes - historySize ;
    lines.erase( lines.begin(), lines.begin() + remove );

    // can't quickly update the dimensions if the fontMetrics aren't uptodate.
    if ( cache.dirty & Cache::FontMetrics ) {
        cache.dirty |= Cache::Dimensions;
        return;
    }

    QVector<int> & lw = cache.fontMetrics.lineWidths;

    assert( lw.size() > remove );
    lw.erase( lw.begin(), lw.begin() + remove );

    if ( cache.dirty & Cache::Dimensions )
        return;

    if ( cache.dimensions.indexOfLongestLine >= remove )
        cache.dimensions.indexOfLongestLine -= remove;
    else
        cache.dirty |= Cache::Dimensions;
}

static inline void set_scrollbar_properties( QScrollBar & sb, int document, int viewport, int singleStep, Qt::Orientation o )
{
    const int min = 0;
    const int max = std::max( 0, document - viewport );
    const int value = sb.value();
    const bool wasAtEnd = value == sb.maximum();
    sb.setRange( min, max );
    sb.setPageStep( viewport );
    sb.setSingleStep( singleStep );
    sb.setValue( o == Qt::Vertical && wasAtEnd ? sb.maximum() : value );
}

void QtMessageLogWidgetPrivate::updateScrollRanges()
{
    updateCache();

    if ( QScrollBar * const sb = q_ptr->verticalScrollBar() ) {
        const int document = lines.size() * cache.fontMetrics.lineSpacing ;
        const int viewport = q_ptr->viewport()->height();
        const int singleStep = cache.fontMetrics.lineSpacing;
        set_scrollbar_properties( *sb, document, viewport, singleStep, Qt::Vertical );
    }

    if ( QScrollBar * const sb = q_ptr->horizontalScrollBar() ) {
        const int document = cache.dimensions.longestLineLength;
        const int viewport = q_ptr->viewport()->width();
        const int singleStep = cache.fontMetrics.lineSpacing; // rather randomly chosen
        set_scrollbar_properties( *sb, document, viewport, singleStep, Qt::Horizontal );
    }
}

void QtMessageLogWidgetPrivate::updateGeometry()
{
    QPoint p = -scrollOffset();
    QTransform m;
    m.translate(p.x(), p.y());
    visibleRect = m.inverted().mapRect( q_ptr->rect() );
    linesVisible = visibleLines( visibleRect.top(), visibleRect.bottom() );
    assert( linesVisible.first <= linesVisible.second );
}

void QtMessageLogWidgetPrivate::addPendingLines()
{
    if ( pendingLines.empty() )
        return;

    const unsigned int oldNumLines = lines.size();

    lines += pendingLines;

    // if the cache isn't dirty, we can quickly update it without
    // invalidation:

    if ( !cache.dirty ) {

        // update fontMetrics:
        const QFontMetrics & fm = q_ptr->fontMetrics();
        QVector<int> & lw = cache.fontMetrics.lineWidths;
        lw.reserve( pendingLines.size() );
        for (auto it = pendingLines.begin(); it != pendingLines.end(); ++it) {
            lw.push_back( fm.width( it->text ) );
        }

        // update dimensions:
        const QVector<int>::const_iterator it =
            std::max_element( lw.begin() + oldNumLines, lw.end() );
        if ( *it >= cache.dimensions.longestLineLength ) {
            cache.dimensions.longestLineLength = *it;
            cache.dimensions.indexOfLongestLine = oldNumLines + ( it - lw.begin() );
        }

        cache.fontMetrics.lineSpacing = fm.lineSpacing();
        cache.fontMetrics.ascent = fm.ascent();
#if QT_VERSION < 0x040200
        cache.fontMetrics.averageCharWidth = fm.width( QLatin1Char( 'M' ) );
#else
        cache.fontMetrics.averageCharWidth = fm.averageCharWidth();
#endif
    }

    pendingLines.clear();

    enforceHistorySize();
    updateScrollRanges();
    q_ptr->viewport()->update();
}

inline int QtMessageLogWidgetPrivate::lineByYCoordinate( int y ) const
{
    updateCache();
    if ( cache.fontMetrics.lineSpacing == 0 )
        return -1;
    const int raw = y / cache.fontMetrics.lineSpacing ;
    if ( raw < 0 )
        return -1;
    if ( raw >= lines.size() )
        return lines.size() - 1;
    return raw;
}

static inline int get_scrollbar_offset( const QScrollBar * sb ) {
    return sb ? sb->value() : 0 ;
}

inline QPoint QtMessageLogWidgetPrivate::scrollOffset() const {
    return QPoint( get_scrollbar_offset( q_ptr->horizontalScrollBar() ),
                   get_scrollbar_offset( q_ptr->verticalScrollBar() ) );
}










QtMessageLogWidget::QtMessageLogWidget( QWidget * parent ) :
    QAbstractScrollArea( parent ),
    d_ptr( new QtMessageLogWidgetPrivate( this ) )
{
    setMouseTracking(true);
    setStyleSheet("QWidget{background:#FFFFFF;}"); //set background color
}


QtMessageLogWidget::~QtMessageLogWidget()
{
}

void QtMessageLogWidget::setHistorySize( unsigned int hs )
{
    Q_D(QtMessageLogWidget);
    if ( hs == d->historySize )
        return;
    d->historySize = hs;
    d->enforceHistorySize();
    d->updateScrollRanges();
    viewport()->update();
}

unsigned int QtMessageLogWidget::historySize() const
{
    Q_D(const QtMessageLogWidget);
    return d->historySize;
}


QString QtMessageLogWidget::text() const
{
    Q_D(const QtMessageLogWidget);
    QString result;
    // reserve space
    result.reserve((d->lines.size() + d->pendingLines.size()) * qMin(512, d->cache.dimensions.longestLineLength / 4));
    for (auto it = d->lines.begin(); it != d->lines.end(); ++it) {
        result += it->text;
        result += '\n';
    }
    for (auto it = d->pendingLines.begin(); it != d->pendingLines.end(); ++it) {
        result += it->text;
        result += '\n';
    }
    result.remove(result.size()-1, 1); // erase last '\n'
    return result;
}


void QtMessageLogWidget::setMinimumVisibleLines( unsigned int num )
{
    Q_D(QtMessageLogWidget);
    if ( num == d->minimumVisibleLines )
        return;
    d->minimumVisibleLines = num;
    updateGeometry();
}

unsigned int QtMessageLogWidget::minimumVisibleLines() const
{
    Q_D(const QtMessageLogWidget);
    return d->minimumVisibleLines;
}


void QtMessageLogWidget::setMinimumVisibleColumns( unsigned int num )
{
    Q_D(QtMessageLogWidget);
    if ( num == d->minimumVisibleColumns )
        return;
    d->minimumVisibleColumns = num;
    updateGeometry();
}

unsigned int QtMessageLogWidget::minimumVisibleColumns() const
{
    Q_D(const QtMessageLogWidget);
    return d->minimumVisibleColumns;
}

void QtMessageLogWidget::setAlternatingRowColors( bool on )
{
    Q_D(QtMessageLogWidget);
    if ( on == d->alternatingRowColors )
        return;
    d->alternatingRowColors = on;
    update();
}

bool QtMessageLogWidget::alternatingRowColors() const
{
    Q_D(const QtMessageLogWidget);
    return d->alternatingRowColors;
}

int QtMessageLogWidget::setupStyle(const QRegularExpression &expr, const QColor &background, const QColor &foreground)
{
    Q_D(QtMessageLogWidget);
    return d->highlighter.setup(expr, background, foreground);
}

int QtMessageLogWidget::removeStyle(int id)
{
    Q_D(QtMessageLogWidget);
    return d->highlighter.remove(id);
}

void QtMessageLogWidget::clearStyles()
{
    Q_D(QtMessageLogWidget);
    return d->highlighter.clear();
}

QSize QtMessageLogWidget::minimumSizeHint() const
{
    Q_D(const QtMessageLogWidget);
    d->updateCache();
    const QSize base = QAbstractScrollArea::minimumSizeHint();
    const QSize view( d->minimumVisibleColumns * d->cache.fontMetrics.averageCharWidth,
                      d->minimumVisibleLines   * d->cache.fontMetrics.lineSpacing );
    const QSize scrollbars( verticalScrollBar() ? verticalScrollBar()->minimumSizeHint().width() : 0,
                horizontalScrollBar() ? horizontalScrollBar()->minimumSizeHint().height() : 0 );
    return base + view + scrollbars;
}

QSize QtMessageLogWidget::sizeHint() const
{
    Q_D(const QtMessageLogWidget);
    if ( d->minimumVisibleLines > 1 || d->minimumVisibleColumns > 1 )
        return minimumSizeHint();
    else
        return 2 * minimumSizeHint();
}


void QtMessageLogWidget::clear()
{
    Q_D(QtMessageLogWidget);
    d->timer.stop();
    d->linesVisible.first = 0;
    d->linesVisible.second = 0;
    d->lines.clear();
    d->pendingLines.clear();
    d->cache.dirty = QtMessageLogWidgetPrivate::Cache::All;
    viewport()->update();
}

void QtMessageLogWidget::textClear()
{
    Q_D(QtMessageLogWidget);
    d->pendingLines.clear();
}

QVector<LineItem> QtMessageLogWidget::getLineItem()
{
    Q_D(QtMessageLogWidget);
    return d->lines;
}
void QtMessageLogWidget::message( const QString & str)
{
    Q_D(QtMessageLogWidget);

    LineItem li;
    li.text = str;
    li.styleID = d->highlighter.highlight(str);
    d->pendingLines.push_back( li );
    d->triggerTimer();
}

void QtMessageLogWidget::message(const QString& str, const QString color)
{
    Q_D(QtMessageLogWidget);
    LineItem li;

    li.text = str;
    li.styleID = d->highlighter.highlight(str);
    li.color = color;

    d->pendingLines.push_back(li);
    d->triggerTimer();
}

void QtMessageLogWidget::message(LineItem li)
{
    Q_D(QtMessageLogWidget);
    d->pendingLines.push_back(li);
    d->triggerTimer();
}

void QtMessageLogWidget::scrollContentsBy(int dx, int dy)
{
    Q_D(QtMessageLogWidget);
    d->updateScrollRanges();
    d->updateCache();
    d->updateGeometry();
    QAbstractScrollArea::scrollContentsBy(dx, dy);
}

void QtMessageLogWidget::mouseMoveEvent(QMouseEvent* event)
{
    /*if (event->pos().y() < 3)
    {
        this->viewport()->setCursor(Qt::SizeVerCursor);
        if (isAjustHieght)
        {
            emit updateCursorPosHeight(CursorDeltaHeight - QCursor::pos().y());
        }
        
    }
    else
    {
        this->viewport()->setCursor(Qt::IBeamCursor);
    }*/
}

void QtMessageLogWidget::mousePressEvent(QMouseEvent* event)
{
    /*if(event->pos().y() < 3 && event->button() == Qt::LeftButton)
    {
        isAjustHieght = true;
        CursorDeltaHeight = QCursor::pos().y();
    }*/
}

void QtMessageLogWidget::mouseReleaseEvent(QMouseEvent* event)
{
   // isAjustHieght = false;
}

void QtMessageLogWidget::paintEvent( QPaintEvent* )
{
    Q_D(QtMessageLogWidget);

    //d->updateCache();

    QPainter p( viewport() );
    p.translate( -d->scrollOffset() );

    const QtMessageLogWidgetPrivate::Cache & cache = d->cache;

    p.setPen( Qt::NoPen );
    p.setBrush( palette().base() );

    if ( d->alternatingRowColors )
    {
        p.drawRect( d->visibleRect );
        p.setBrush( palette().alternateBase() );
        for ( unsigned int i = d->linesVisible.first % 2 ? d->linesVisible.first : d->linesVisible.first+1, end = d->linesVisible.second ; i < end ; i+=2 ) {
            p.drawRect( d->lineRect( i ).adjusted(0, 0, size().width(), 0) );
        }
    } else {
        p.drawRect( d->visibleRect );
    }

    // ### unused optimization: paint lines by styles to minimise pen changes.
    for ( unsigned int i = d->linesVisible.first, end = d->linesVisible.second ; i != end ; ++i )
    {
        const LineItem & li = d->lines[i];

        assert( !li.styleID || d->highlighter.contains( li.styleID ) );

        MessageHighlignter::Style st = d->highlighter.style(li.styleID);
        p.setPen( Qt::NoPen );
        p.setBrush(st.background);
        p.drawRect( d->lineRect( i ).adjusted(0, 0, size().width(), 0) );
        
        if (li.color == "red")
            p.setPen(QColor("#b52f28"));
        else if (li.color == "yellow")
            p.setPen(QColor("#ebb000"));
        else if (li.color == "black")
            p.setPen(QColor("#000000"));
        else if (li.color == "blue")
            p.setPen(QColor("0000ff"));
        p.drawText( 0, i * cache.fontMetrics.lineSpacing + cache.fontMetrics.ascent, li.text );
    }
}

void QtMessageLogWidget::timerEvent( QTimerEvent * e )
{
    Q_D(QtMessageLogWidget);
    if ( e->timerId() == d->timer.timerId() ) {
        d->timer.stop();
        d->addPendingLines();
        d->updateGeometry();
    } else {
        QAbstractScrollArea::timerEvent( e );
    }
}

void QtMessageLogWidget::changeEvent( QEvent * e )
{
    Q_D(QtMessageLogWidget);
    QAbstractScrollArea::changeEvent( e );
    d->cache.dirty |= QtMessageLogWidgetPrivate::Cache::FontMetrics;
    d->updateCache();
    d->updateGeometry();
    update();
}

void QtMessageLogWidget::keyPressEvent(QKeyEvent * e )
{
#ifndef QT_NO_CLIPBOARD
    if (e->matches(QKeySequence::Copy)) {
        QGuiApplication::clipboard()->setText(text());
    }
#endif
    QAbstractScrollArea::keyPressEvent(e);
}

void QtMessageLogWidget::resizeEvent( QResizeEvent * e )
{
    Q_D(QtMessageLogWidget);
    d->updateScrollRanges();
    d->updateCache();
    d->updateGeometry();
    QAbstractScrollArea::resizeEvent(e);
}


LogWidget::LogWidget(QWidget* parent) : QWidget(parent)
{
    m_qVBoxLayout = new QVBoxLayout(this);
    this->setLayout(m_qVBoxLayout);
    m_logWidget = new QtMessageLogWidget();
   
    CreateToolBar();
    m_qVBoxLayout->addWidget(m_logWidget, -1);
    m_qVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_qVBoxLayout->setSpacing(0);
    
}

LogWidget::~LogWidget()
{

}

void LogWidget::SetLogWidgetLayout()
{

}

void LogWidget::message(const QString& msg, const QString color)
{
    m_logWidget->message(msg, color);
}

void LogWidget::CreateToolBar()
{
    QWidget* widget = new QWidget(this);
    widget->setStyleSheet(".QWidget{background-color:rgba(255,255,255); }");
    widget->setMaximumHeight(40);
    QHBoxLayout* qHBoxLayout = new QHBoxLayout();
    qHBoxLayout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(qHBoxLayout);
    m_qVBoxLayout->addWidget(widget);

    QToolBar* ToolBar = new QToolBar(this);
    ToolBar->setIconSize(QSize(20, 20));
    QAction * clear = new QAction(QIcon(":/image/images/routine/clear.png"), "&clear log", this);
    changeLogHeight = new QSlider(Qt::Horizontal, this);
    changeLogHeight->setMaximumWidth(100);
    changeLogHeight->setMinimum(100);
    changeLogHeight->setMaximum(400);
    changeLogHeight->setValue(200);
    changeLogHeight->setToolTip("change log widget height");
    changeLogHeight->setStyleSheet("QSlider::handle:horizontal {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #555555, stop:1 #555555);"
        "width: 5px;"
        "height: 20px;"
        "border-radius:2px;}");
    ToolBar->addAction(clear);
    ToolBar->addSeparator();
    ToolBar->addWidget(changeLogHeight);
    ToolBar->addSeparator();
    logFilter = new QLineEdit(this);
    logFilter->setStyleSheet("QLineEdit { border: 1px solid black; border-right: none;border-left: none; }");
    QPushButton* fmark = new QPushButton(QIcon(":/image/images/routine/search.png"),"");
    fmark->setCheckable(false);
    fmark->setStyleSheet("QPushButton{border: 1px solid black; border-right: none;background:transparent;}");
    clearBtn = new QPushButton(QIcon(":/image/images/routine/temp.png"),"");
    clearBtn->setStyleSheet("QPushButton { border: 1px solid black; border-left: none; background-color: transparent; }");
    
    ToolBar->addWidget(fmark);
    ToolBar->addWidget(logFilter);
    ToolBar->addWidget(clearBtn);
    qHBoxLayout->addWidget(ToolBar);
    qHBoxLayout->addStretch();
    //connect(changeLogHeight, SIGNAL(valueChanged(int)), this, SLOT(changeLogWidgetHeightSlot(int)));
    connect(changeLogHeight, SIGNAL(sliderReleased()), this, SLOT(changeLogWidgetHeightSlot()));
    connect(clear, SIGNAL(triggered()), m_logWidget, SLOT(clear()));
    connect(logFilter, &QLineEdit::textChanged,this,&LogWidget::logFilterTextChanged);
    connect(logFilter, &QLineEdit::editingFinished, this, &LogWidget::filterLog);
    //connect(fmark, &QPushButton::clicked, this,&LogWidget::filterLog);
    connect(clearBtn, &QPushButton::clicked,this,&LogWidget::reopenLog);
    
}
void LogWidget::reopenLog()
{
    if (newLogWidget)
    {
        m_qVBoxLayout->removeWidget(newLogWidget);
        delete newLogWidget;
        newLogWidget = nullptr;
    }

    m_qVBoxLayout->update();
    m_qVBoxLayout->addWidget(m_logWidget);
}
void LogWidget::filterLog()
{
    QString keyword = logFilter->text();
    if (m_keyword == keyword) return;
    QVector<LineItem> content = m_logWidget->getLineItem();
    newLogWidget = new QtMessageLogWidget();
    for (size_t i = 0; i < content.size(); i++)
    {
        LineItem line = content[i];
        if (line.text.toLower().contains(keyword.toLower()))
        {
            newLogWidget->message(line);
        }
    }
    m_keyword = keyword;
    m_qVBoxLayout->removeWidget(m_logWidget);
    m_qVBoxLayout->addWidget(newLogWidget);
}
void LogWidget::logFilterTextChanged(QString curText)
{
    if (curText == "")
    {
        clearBtn->setIcon(QIcon(":/image/images/routine/temp.png"));
        logFilter->clear();
        m_keyword.clear();
    }
    else
        clearBtn->setIcon(QIcon(":/image/images/routine/clearText.png"));
    reopenLog();
}
void LogWidget::changeLogWidgetHeightSlot()
{
    emit changeLogWidgetHeight(changeLogHeight->value());
}
