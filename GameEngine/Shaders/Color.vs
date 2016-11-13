//filename: Color.vs
/////////////////////

//Globals
cbuffer MatrixBuffer
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjectionMatrix;
};

//Typedefs
struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position: SV_POSITION;
	float4 color : COLOR;
};

//VertexShader
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;

	//Change the position vector to be 4 for proper calculations.
	input.position.w = 1.0f;

	//Calc position of the vertex against world view and projection matricies
	output.position = mul(input.position, WorldMatrix);
	output.position = mul(output.position, ViewMatrix);
	output.position = mul(output.position, ProjectionMatrix);

	//Store input color for pixel shader to use
	output.color = input.color;
	
	return output;

}