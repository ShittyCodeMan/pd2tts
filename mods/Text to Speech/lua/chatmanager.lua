local mod_path = mod_path or ModPath
local _STSpeech = assert(package.loadlib(mod_path .. "bin/pd2tts.dll", "luaSTSpeech"))

function STSpeech(message)
	local f = io.open( mod_path .. "bin/msg.dat", "w" )
	if f then
		f:write(message)
		f:close()
		_STSpeech()
	end
end

Hooks:PostHook(ChatGui, "receive_message", "tts_receive_message", function(self, name, message, ...)
	STSpeech(message)
end )

STSpeech("Speech ready")
