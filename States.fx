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

BlendState bsTransparent
{
  AlphaToCoverageEnable = false;
  BlendEnable[0] = true;
  SrcBlend = Src_Alpha;
  DestBlend = Inv_Src_Alpha;
  BlendOp = Add;
  SrcBlendAlpha = One;
  DestBlendAlpha =Zero;
  BlendOpAlpha = Add;
  //RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
};