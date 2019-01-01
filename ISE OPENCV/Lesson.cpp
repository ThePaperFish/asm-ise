
#include "stdafx.h"

/*

============== L10 - Character recognition  =====================================
	
	OCR (optical character recognition)
	- TesserAct



============== L10 - Representation  =====================================

send Feature Extension instead of raw image to recognize image

-> Boundary Based: Shape
	
	-Chain Code Direction - 15%

		^				1
	< -	+ -	>		2 -	+ -	0
		v				3

	- Clockwise / Anti-Clockwise
	- 4 Direction / 8 Direction


	  ******   |  represent by chain code
	  *    *   |  [00000333223322211111]
	  *    *   |  
	  *  ***   |  -> convert   (regardless of starting point)
	  *  *	   |  [00003003010300300003] - 00003 003 01 03 003 00003
	  ****	   |  						   ^				   ^
			   |  -> rearrange (0003 0003 < 0003 003)
			   |  [00003000030030103003]

	Practice
	[0076665443221]
	[0770077077077]
	[0077077077077]

-> Region Based: Shape / Colour / Texture

	


============== L9 - Segmentation =====================================

Segmentation
- Threshold
- Texture
- color range

Connected Component Labelling CCL 8%
	1. find the white pixels
	2. check up & left
		- if exist label, get the lowest label
		- if not, create new label
	3. check bottom & right
		- to merge segmentation

OTSU - find value to threshold

Grey -> H Grey -> OTSU -> Binary -> Seperate Character

============== L8 - Morphology 20% =====================================

Morphology (Structure Element = mask)
- Dilation ⨁
  -> connect
  - OR / HIT
  -	|		*		|
	|	*	*	*	|
	|		*		|
	Check its neighbour
	if one = white then become white

- Eroson ⊖
  -> disconnect
  - AND / FIT
  -	|		*		|
	|	*	*	*	|
	|		*		|
	Check its neighbour
	if one = black then become black

- Opening
  - Eroson -> Dilation

- Closing
  - Dilation -> Eroson 

-------------------------------------------------

Morphology
-> normally in binary
-> Grayscale 
	-> get Max Value
	-> no one use in real life

============== L7 - Edge Detection 20% =====================================

2 Way to Binarize:
- Threshold ( x => value )
- Sharpening Filter -> Derivative
  - Sobel (Always exclude border - dunno why)
	
   Horizontal Gx	|	Vertical Gy					
					|					
	-1	-2	-1		|	-1	 0	 1		
	 0	 0	 0		|	-2	 0	 2		
	 1	 2	 1		|	-1	 0	 1		

	Advantage		: Edge are thick and fine
	Disadvantage	: Edge not enough for edge detection

  - Laplacian Operator (combination of x & y)

	 0	  1	   0	
	 1	 -4	   1	
	 0	  1	   0

============== Math Function 10% =====================================

============== L5 - Masking / Convolution 15% =====================================

Objective : to elimate ridiculous pixel [0,0,1,2,0,99,0,3,1]
Purpose : Blur / Sharpening / Edge Detection

Methods
|-> Exclude Border		- no add border
|-> Zero Border			- add zeros
|-> Border Reflection	- add border's value

Filter
|-> Average	
|-> Min		
|-> Max		
|-> Median	

Default Mask:	*	*	*
				*	*	*
				*	*	*

Image with method(Zero) * Division(Mask) = Better Image

============== L4 - Image Enhancement - Equalize Histogram 15%====================

Grayscale Value (GV)
|-> Count of each Pixel GV
|-|-> Probability of each GV
|-|-|-> Accumulate of Probability of GV
|-|-|-|-> (NumGV - 1) * AccGV 
|-|-|-|-|-> RoundUp = > New Value <

Done!

*/