var launch_priv = require('./build/Release/launch_priv'),
    process = require('process');

module.exports = {
  ensurePriviledged: function() {
    console.log('Current uid: ' + process.getuid());
    try {
      process.setuid(0);
      console.log('New uid: ' + process.getuid());
    }
    catch (err) {
     console.log('Failed to set uid: ' + err);
    }
    var cmd_line = process.execPath;
    var args = process.argv.slice(1).concat(process.execArgv);
    console.log('cmd_line: ' + cmd_line + ' ' + args.join());
    if(process.getuid() != 0) {
      var result = launch_priv.launchPriv(cmd_line, args);
      console.log('result:' + result);
      if(result === 0) {
        process.exit(0);
      }else{
        console.log('ERROR!');
      }
    }else{
      console.log('ALLREADY ROOT!');
    }
  },
};
