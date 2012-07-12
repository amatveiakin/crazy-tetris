RasterizerState rsSemicubes 
{
  FillMode = Solid; 
  CullMode = None;
  AntiAliasedLineEnable = true;
  MultisampleEnable = true;
};

RasterizerState rsCullBack 
{
  FillMode = Solid; 
  CullMode = Back;
  AntiAliasedLineEnable = true;
  MultisampleEnable = true;
};

RasterizerState rsCullFront 
{
  FillMode = Solid; 
  CullMode = Front;
  AntiAliasedLineEnable = true;
  MultisampleEnable = true;
};