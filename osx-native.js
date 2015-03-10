/*jslint node: true*/
'use strict';

var launch_priv = require('./build/Release/launch_priv'),
    debuglog = require('util').debuglog('osx-native');

module.exports = {
    launchPriviledged: function (cmd, args, progress, callback) {
        debuglog('Current uid: ' + process.getuid());
        try {
            process.setuid(0);
            debuglog('New uid: ' + process.getuid());
        } catch (err) {
            debuglog('Failed to set uid: ' + err);
        }
        if (process.getuid() !== 0) {
            var cmd_line = cmd || process.execPath,
                cmd_args = args || process.argv.slice(1).concat(process.execArgv),
                outputBuff = new Buffer(0),
                result = launch_priv.launchPriv(cmd_line, cmd_args, function (buff) {
                    debuglog('received: ' + buff);
                    if (typeof progress === 'function') {
                        progress(buff);
                    }
                }, function () {
                    debuglog('Done.');
                    if (typeof callback === 'function') {
                        callback(null);
                    }
                });

            debuglog('cmd_line: ' + cmd_line + ' ' + cmd_args.join());
            debuglog('result:' + result);
            if (result !== 0) {
                debuglog('ERROR: ' + result);
                if (typeof callback === 'function') {
                    callback(result);
                }
            }
        } else {
            debuglog('ALLREADY ROOT!');
        }
    }
};
