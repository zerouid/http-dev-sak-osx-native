var osx = require('../osx-native');

// osx.launchPriviledged();
console.log(osx.getProxySettings());
console.log(osx.setProxySettings({
  HTTPProxyEnabled: true,
  HTTPProxyHost: "127.0.0.1",
  HTTPProxyPort: 8080,
}));
console.log(osx.getProxySettings());
