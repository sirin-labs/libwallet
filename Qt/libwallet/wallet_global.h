#ifndef WALLET_GLOBAL_H
#define WALLET_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WALLET_LIBRARY)
#define WALLETSHARED_EXPORT Q_DECL_EXPORT
#else
#define WALLETSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // WALLET_GLOBAL_H
