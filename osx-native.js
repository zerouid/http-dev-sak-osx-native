/*jslint node: true*/
'use strict';

var launch_priv = require('./build/Release/launch_priv'),
    util = require('util'),
    events = require('events'),
    debuglog = require('util').debuglog('osx-native');

function OSXLauncher() {
    if (!(this instanceof OSXLauncher))
        return new OSXLauncher();

    events.EventEmitter.call(this);
}

util.inherits(OSXLauncher, events.EventEmitter);

OSXLauncher.prototype.launchPriviledged = function (cmd, args) {
    var self = this;
    debuglog('Current uid: ' + process.getuid());
    var cmd_line = cmd || process.execPath,
        cmd_args = args || [];

    debuglog('cmd_line: ' + cmd_line + ' ' + cmd_args.join());
    var result = launch_priv.launchPriv(cmd_line, cmd_args, function (buff) {
            debuglog('Received: ' + buff);
            self.emit('data', buff);
        }, function () {
            debuglog('End.');
            self.emit('end');
        });

    debuglog('result:' + result);
    if (result === 0) {
        debuglog('Started.');
        self.emit('start');
    }else{
        debuglog('ERROR: ' + result);
        self.emit('error', result);
    }
};

module.exports = new OSXLauncher();
