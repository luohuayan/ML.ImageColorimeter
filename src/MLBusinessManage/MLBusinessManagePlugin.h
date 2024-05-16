#ifndef SRC_MLBUSINESSMANAGE_MLBUSINESSMANAGEPLUGIN_H_
#define SRC_MLBUSINESSMANAGE_MLBUSINESSMANAGEPLUGIN_H_


#include <QtPlugin>

#include "mlbusinessmanage_global.h"
#include "service/Result.h"

class MLBUSINESSMANAGE_EXPORT MLBusinessManagePlugin {
 public:
    virtual ~MLBusinessManagePlugin() {}
};
// Plug-in identifier
#define AbstractInterface_iid "MLBusinessManagePlugin"
// Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(MLBusinessManagePlugin, AbstractInterface_iid)

#endif  // SRC_MLBUSINESSMANAGE_MLBUSINESSMANAGEPLUGIN_H_