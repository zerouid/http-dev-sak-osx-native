var launch_priv = require('./build/Release/launch_priv');
function osx() {
	this.launchPriv = launch_priv.launchPriv;
}
module.exports = osx;
