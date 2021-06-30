# svencoop-spray-size
Calculates optimal spray and image sizes for the HL1 mod, Sven Co-op
To be executed from the command prompt as there's no user interface (Windows users beware! xd)

Based on these spray rules:
  - Spray ratio a/b has to follow a * b <= 56
  - Spray size has to be <= 14336 pcs (256 * 56)
  - Spray has to be a 256-color bitmap image (.bmp)
  - Spray's sides (by extension, the original image's sides) have to be multiples of 16

It offers you to edit your image slightly if the ratio is improper, prioritizing minimal symmetric difference of the original and the edited image
(aka maximal overlap). Otherwise it gives you a list of spray sizes to scale down to and a list of sizes to edit your image to to get a better ratio.
