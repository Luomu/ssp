-- Helpful stuff & shortcuts for development

--god mode on/off
function ocp (on)
	if on then
		Game.player:SetInvulnerable(1)
		Comms.Message("God mode on")
	else
		Game.player:SetInvulnerable(0)
		Comms.Message("God mode off")
	end
end

-- kill current combat target
function kill()
	local ct = Game.player:GetCombatTarget()
	if ct then
		ct:Explode()
	end
end

-- quick combat test (also makes you invulnerable)
function en()
	Comms.Message("Prepare to die", "Enemy")
	Dev.SpawnEnemy()
	ocp(1)
end

-- spawn harmless drone
td = function()
	Comms.Message("Prepare to not die", "TD")
	Dev.SpawnTargetDrone()
end
