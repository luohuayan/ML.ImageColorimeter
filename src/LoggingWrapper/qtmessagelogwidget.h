#ifndef QTMESSAGELOGWIDGET_H
#define QTMESSAGELOGWIDGET_H
#include <QWidget>
#include <QScopedPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QBasicTimer>
#include <QRegularExpression>
#include <QScrollBar>



#if QT_VERSION < 0x050000
#include <QtGui/QAbstractScrollArea>
#else
#include <QtWidgets/QAbstractScrollArea>

#endif

QT_BEGIN_NAMESPACE
class QString;
class QStringList;
class QColor;
class QRegularExpression;
QT_END_NAMESPACE

struct LineItem {
    QString text;
    unsigned int styleID;
    QString color;
};
class MessageHighlignter
{
public:
    struct Style
    {
        QRegularExpression expr;
        QColor background;
        QColor foreground;

        Style() {}
        Style(const QColor& c) :
            background(Qt::transparent), foreground(c) {}

        Style(const QRegularExpression& e, const QColor& bg, const QColor& fg) :
            expr(e), background(bg), foreground(fg) {}

        void SetForeground(QColor foreground_)
        {
            foreground = foreground_;
        }
    };

    MessageHighlignter() {}
    ~MessageHighlignter() {}

    bool contains(int id) const {
        return (id > 0 && id <= styles.size());
    }

    int setup(const QRegularExpression& expr, const QColor& background, const QColor& foreground)
    {
        styles.push_back(Style(expr, background, foreground));
        return styles.size();
    }

    int remove(int id)
    {
        if (id > 0 && id < styles.size()) {
            styles.removeAt(id - 1);
            return 1;
        }
        return 0;
    }

    int highlight(const QString& text) const
    {
        QRegularExpressionMatch match;
        int i = 1;
        for (auto it = styles.begin(); it != styles.end(); ++it, ++i) {
            match = it->expr.match(text);
            if (match.hasMatch())
                return i;
        }
        return 0;
    }

    Style style(const int id) const {
        static Style defaultStyle(Qt::black);
        if (id > 0 && id <= styles.size()) {
            return styles[id - 1];
        }
        return defaultStyle;
    }

    void clear() {
        styles.clear();
    }

private:
    QList<Style> styles;
};

class  QtMessageLogWidget :
    public QAbstractScrollArea
{
    Q_OBJECT
        Q_PROPERTY(uint historySize READ historySize WRITE setHistorySize)
        Q_PROPERTY(QString text READ text)
        Q_PROPERTY(uint minimumVisibleLines READ minimumVisibleLines WRITE setMinimumVisibleLines)
        Q_PROPERTY(uint minimumVisibleColumns READ minimumVisibleColumns WRITE setMinimumVisibleColumns)
        Q_PROPERTY(bool alternatingRowColors READ alternatingRowColors WRITE setAlternatingRowColors)

public:
    /*!
     * Constructor. Creates an empty QtMessageLogWidget.
     */
    explicit QtMessageLogWidget(QWidget* parent = Q_NULLPTR);
    /*!
     * Destructor.
     */
    ~QtMessageLogWidget();


    /*!
     * \property QtMessageLogWidget::historySize
     *
     * Specifies the maximum number of lines this widget will hold before
     * dropping old lines. The default is INT_MAX (ie. essentially unlimited).
     * Get this property's value using %historySize(), and set it with
     * %setHistorySize().
     */
    void setHistorySize(unsigned int size);
    unsigned int historySize() const;

    /*!
     * \property QtMessageLogWidget::text
     *
     * Contains the current %text as a single string.
     */
    QString text() const;
    QVector<LineItem> getLineItem();
    /*!
     * \property QtMessageLogWidget::minimumVisibleLines
     * Specifies the number of lines that should be visible at any one
     * time. The default is 1 (one).
     * Get this property's value using %minimumVisibleLines(), and set it
     * using %setMinimumVisibleLines().
     */
    void setMinimumVisibleLines(unsigned int num);
    unsigned int minimumVisibleLines() const;
    /*!
     * \property QtMessageLogWidget::minimumVisibleColumns
     *
     * Specifies the number of columns that should be visible at any one
     * time. The default is 1 (one). The width is calculated using
     * QFontMetrics::averageCharWidth(), if that is available. Otherwise,
     * the width of \c M is used.
     * Get this property's value using %minimumVisibleColumns(), and set it
     * using %setMinimumVisibleColumns().
     */
    void setMinimumVisibleColumns(unsigned int num);
    unsigned int minimumVisibleColumns() const;

    /*!
     * \property QtMessageLogWidget::alternatingRowColors
     *
     * Specifies whether the background should be drawn using
     * row-alternating colors. The default is \c false.
     * Get this property's value using %alternatingRowColors(), and set it
     * using %setAlternatingRowColors().
     */
    void setAlternatingRowColors(bool on);
    bool alternatingRowColors() const;

    int setupStyle(const QRegularExpression& expr, const QColor& background, const QColor& foreground);
    int removeStyle(int id);
    void clearStyles();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;


signals:
    void updateCursorPosHeight(int height);

private:
    /*bool isAjustHieght = false;
    int CursorDeltaHeight = 0;*/
public Q_SLOTS:
    /*!
     * Clears the text.
     */
    void clear();
    void textClear();
    /*!
     * Appends \a str to the view, highlighting the line
     */
    virtual void message(const QString& msg);

    void message(const QString& msg, const QString color);

    void message(LineItem line);

    /*protected:
        virtual void highlight( const QString& text );*/

        // QAbstractScrollArea interface
protected:
    virtual void scrollContentsBy(int dx, int dy);

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    virtual void timerEvent(QTimerEvent*) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent*) Q_DECL_OVERRIDE;
    virtual void changeEvent(QEvent*) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent*) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
private:
    QScopedPointer<class QtMessageLogWidgetPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtMessageLogWidget)
};
class QtMessageLogWidgetPrivate
{
    friend class QtMessageLogWidget;
    QtMessageLogWidget* q_ptr;
public:
    explicit QtMessageLogWidgetPrivate(QtMessageLogWidget* qq);
    ~QtMessageLogWidgetPrivate();

    void updateCache() const;

    void triggerTimer() {
        if (!timer.isActive())
            timer.start(25, q_ptr);
    }

    void addPendingLines();
    void enforceHistorySize();
    void updateScrollRanges();

    void updateGeometry();

    inline QPair<int, int> visibleLines(int top, int bottom) {
        return qMakePair(qMax(0, lineByYCoordinate(top)),
            qMax(0, 1 + lineByYCoordinate(bottom)));
    }

    inline int lineByYCoordinate(int x) const;

    inline QPoint scrollOffset() const;

    inline QRect lineRect(int idx) const
    {
        assert(!cache.dirty);
        return QRect(0, idx * cache.fontMetrics.lineSpacing,
            cache.dimensions.longestLineLength,
            cache.fontMetrics.lineSpacing - 1);
    }

    //unsigned int findOrAddStyle( const Style & style );

private:
    mutable struct Cache {
        enum {
            Dimensions = 1,
            FontMetrics = 2,
            All = FontMetrics | Dimensions
        };
        Cache() : dirty(All) {}
        int dirty;

        struct {
            int lineSpacing;
            int ascent;
            int averageCharWidth;
            QVector<int> lineWidths;
        } fontMetrics;

        struct {
            int indexOfLongestLine;
            int longestLineLength;
        } dimensions;
    } cache;

    //QHash<unsigned int, Style> styleByID;
    //QHash<Style,unsigned int> idByStyle;
    MessageHighlignter highlighter;

    QVector<LineItem> lines, pendingLines;

    QRect visibleRect;
    QPair<int, int> linesVisible;

    unsigned int historySize;
    unsigned int minimumVisibleLines;
    unsigned int minimumVisibleColumns;

    bool alternatingRowColors;

    QBasicTimer timer;
};

class LogWidget : public QWidget
{
    Q_OBJECT
public:
    LogWidget(QWidget* parent = nullptr);
    ~LogWidget();

public:
    void SetLogWidgetLayout();
    void message(const QString& msg, const QString color);
private:
    void CreateToolBar();

public slots:
    void changeLogWidgetHeightSlot();
    void reopenLog();
    void filterLog();
    void logFilterTextChanged(QString curText);
signals:
    void changeLogWidgetHeight(int);
private:
    //QHBoxLayout* m_qHBoxLayout;
    QVBoxLayout* m_qVBoxLayout;
    QtMessageLogWidget* m_logWidget = nullptr,* newLogWidget = nullptr;
    QSlider* changeLogHeight;
    QLineEdit* logFilter;
    QPushButton* clearBtn;
    QStringList filtRes;
    QString m_keyword = "";
};



#endif // QTMESSAGELOGWIDGET_H
