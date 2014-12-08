/*
* Copyright (c) 2013 BlackBerry Limited
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

var audioMetaData,
	resultObjs = {},
	threadCallback = null,
	processing = false,
   _utils = require("../../lib/utils");

module.exports = {

	// Code can be declared and used outside the module.exports object,
	// but any functions to be called by client.js need to be declared
	// here in this object.

	audioMetaDataGetMetaData: function (success, fail, args, env) {
		if (!processing) {
			processing = true;
			var result = new PluginResult(args, env);
			resultObjs[result.callbackId] = result;
			var value;
			if (args && args["input"]) {
				value = JSON.parse(decodeURIComponent(args["input"]));
				audioMetaData.getInstance().audioMetaDataGetMetaData(result.callbackId, value);
				result.noResult(true);
			}
		}
	},

	audioMetaDataSetTagData: function (success, fail, args, env) {
		if (!processing) {
			processing = true;
			var result = new PluginResult(args, env);
			resultObjs[result.callbackId] = result;
			var value;
			if (args && args["input"]) {
				value = JSON.parse(decodeURIComponent(args["input"]));
				audioMetaData.getInstance().audioMetaDataSetTagData(result.callbackId, value);
				result.noResult(true);
			}
		}
	},
	
	audioMetaDataRemoveTag: function (success, fail, args, env) {
		if (!processing) {
			processing = true;
			var result = new PluginResult(args, env);
			resultObjs[result.callbackId] = result;
			var value;
			if (args && args["input"]) {
				value = JSON.parse(decodeURIComponent(args["input"]));
				audioMetaData.getInstance().audioMetaDataRemoveTag(result.callbackId, value);
				result.noResult(true);
			}
		}
	}
};

///////////////////////////////////////////////////////////////////
// JavaScript wrapper for JNEXT plugin for connection
///////////////////////////////////////////////////////////////////

JNEXT.AudioMetaData = function () {
	var self = this,
		hasInstance = false;

	self.getId = function () {
		return self.m_id;
	};

	self.init = function () {
		if (!JNEXT.require("libAudioMetaData")) {
			return false;
		}

		self.m_id = JNEXT.createObject("libAudioMetaData.AudioMetaData_JS");

		if (self.m_id === "") {
			return false;
		}

		JNEXT.registerEvents(self);
	};

	// calls into InvokeMethod(string command) in audiometadata_js.cpp
	
	//Gets the metadata of the mp3 passed in
	self.audioMetaDataGetMetaData = function (callbackId, input) {
		return JNEXT.invoke(self.m_id, "audioMetaDataGetMetaData " + callbackId + " " + input);
	};
	
	//Sets metadata given user input
	self.audioMetaDataSetTagData = function (callbackId, input) {
		return JNEXT.invoke(self.m_id, "audioMetaDataSetTagData " + callbackId + " " + JSON.stringify(input));
	};
	
	//Removes metadata from file
	self.audioMetaDataRemoveTag = function (callbackId, input) {
		return JNEXT.invoke(self.m_id, "audioMetaDataRemoveTag " + callbackId + " " + input);
	};
	
	// Fired by the Event framework (used by asynchronous callbacks)
	self.onEvent = function (strData) {
		var arData = strData.split(" "),
			callbackId = arData[0],
			result = resultObjs[callbackId],
			data = arData.slice(1, arData.length).join(" ");

		processing = false;
		if (result) {
			if (callbackId != threadCallback) {
				result.callbackOk(data, false);
				delete resultObjs[callbackId];
			} else {
				result.callbackOk(data, true);
			}
		}
	};

	// ************************
	// End of methods to edit
	// ************************
	self.m_id = "";

	self.getInstance = function () {
		if (!hasInstance) {
			hasInstance = true;
			self.init();
		}
		return self;
	};

};

audioMetaData = new JNEXT.AudioMetaData();
