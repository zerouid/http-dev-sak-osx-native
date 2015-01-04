/*jslint node: true*/
'use strict';

var launch_priv = require('./build/Release/launch_priv');

module.exports = {
    launchPriviledged: function (cmd, args, success_callback, fail_callback) {
        console.log('Current uid: ' + process.getuid());
        try {
            process.setuid(0);
            console.log('New uid: ' + process.getuid());
        } catch (err) {
            console.log('Failed to set uid: ' + err);
        }
        if (process.getuid() !== 0) {
            var cmd_line = cmd || process.execPath,
                cmd_args = args || process.argv.slice(1).concat(process.execArgv),
                result = launch_priv.launchPriv(cmd_line, cmd_args);

            console.log('cmd_line: ' + cmd_line + ' ' + cmd_args.join());
            console.log('result:' + result);
            if (result === 0) {
                if (typeof success_callback === 'function') {
                    success_callback();
                } else {
                    process.exit(0);
                }
            } else {
                console.log('ERROR!');
                if (typeof fail_callback === 'function') {
                    fail_callback(result);
                }
            }
        } else {
            console.log('ALLREADY ROOT!');
        }
    }
};
