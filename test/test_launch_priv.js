/*jslint node: true*/
'use strict';

var osx = require('../osx-native');

var args = [
    '-e',
    'console.log("uid:" + process.getuid());process.setuid(0);console.log("uid:" + process.getuid());console.error("this is an error");setTimeout(function(){console.log("later...");},1000);'
];

osx.on('start', function() { console.log('started'); });
osx.on('data', function(buff) { console.log('rceived data:' + buff.toString()); });
osx.on('end', function() { console.log('stopped'); });
osx.on('error', function(err) { console.log('ooops:'+err); });
osx.launchPriviledged(process.execPath, args);
