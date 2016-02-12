local mod_path = mod_path or ModPath
_G._STSpeech = _G._STSpeech or assert(package.loadlib(mod_path .. "bin/pd2tts.dll", "luaSTSpeech"))

function STSpeech(message)
	local function Sanitize(msg)
		local r = ""
		for s in msg:gmatch("[%w%p]+") do
			r = r .. s .. " "
		end
		return r
	end
	
	local f = io.open( mod_path .. "bin/msg.dat", "w" )
	if f then
		f:write(Sanitize(message))
		f:close()
		_STSpeech()
	end
end

Hooks:PostHook(ChatGui, "receive_message", "tts_receive_message", function(self, name, message, ...)
	STSpeech(message)
end )

STSpeech("Speech ready")
