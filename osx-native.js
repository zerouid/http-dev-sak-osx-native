var launch_priv = require('./build/Release/launch_priv'),
    process = require('process');

module.exports = {
  launchPriviledged: function(cmd, arguments, success_callback, fail_callback) {
    console.log('Current uid: ' + process.getuid());
    try {
      process.setuid(0);
      console.log('New uid: ' + process.getuid());
    }
    catch (err) {
     console.log('Failed to set uid: ' + err);
    }
    var cmd_line = cmd || process.execPath;
    var args = arguments || process.argv.slice(1).concat(process.execArgv);
    console.log('cmd_line: ' + cmd_line + ' ' + args.join());
    if(process.getuid() != 0) {
      var result = launch_priv.launchPriv(cmd_line, args);
      console.log('result:' + result);
      if(result === 0) {
        if(typeof success_callback === 'function') {
          success_callback();
        }else{
          process.exit(0);
        }
      }else{
        console.log('ERROR!');
        if(typeof fail_callback === 'function') {
          success_callback();
        }
      }
    }else{
      console.log('ALLREADY ROOT!');
    }
  },
  getProxySettings: launch_priv.getProxySettings,
  setProxySettings: launch_priv.setProxySettings,
};
