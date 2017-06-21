require("Resources.Scenes.Test.inputs")

local entCount = 0;

Start = function()
    Level:BindFunctionToKey(65, OnKeyA, true);
    Level:BindFunctionToKey(68, OnKeyD, true);
    Level:BindFunctionToKey(83, OnKeyS, true);
    Level:BindFunctionToKey(87, OnKeyW, true);
    Level:BindFunctionToKey(69, OnKeyE, true);
    Level:BindFunctionToKey(81, OnKeyQ, true);
    Level:BindFunctionToKey(89, OnKeyY, true);
    Level:BindFunctionToKey(72, OnKeyH, true);
    Level:BindFunctionToKey(85, OnKeyU, true);
    Level:BindFunctionToKey(74, OnKeyJ, true);
    Level:BindFunctionToKey(73, OnKeyI, true);
    Level:BindFunctionToKey(75, OnKeyK, true);
    Level:BindFunctionToKey(79, OnKeyO, true);
    Level:BindFunctionToKey(76, OnKeyL, true);
    Level:BindFunctionToKey(88, OnKeyX, false);
    Level:BindFunctionToKey(90, OnKeyZ, false);
    Level:BindFunctionToKey(32, OnKeySpace, false);
end

GameLoop = function()

end

OnKeyX = function()
    print("Entity count: " .. Level:GetEntityCount());
end

OnKeyZ = function()
    local pos = Level:GetCamera():GetPosition();
    local dir = Level:GetCamera():GetDirection();
    pos.x = pos.x + dir.x * 2;
    pos.y = pos.y + dir.y * 2;
    pos.z = pos.z + dir.z * 2;
    local ent = Level:Spawn("test" .. entCount, "m3d_deer", pos);
    entCount = entCount + 1;
    ent:Scale(vec3(0.1, 0.1, 0.1));
end
