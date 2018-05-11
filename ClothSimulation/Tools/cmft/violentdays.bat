cmft   %* --inputFacePosX violentdays_rt.tga ^
          --inputFaceNegX violentdays_lf.tga ^
          --inputFacePosY violentdays_up.tga ^
          --inputFaceNegY violentdays_dn.tga ^
          --inputFacePosZ violentdays_bk.tga ^
          --inputFaceNegZ violentdays_ft.tga ^
          ::Filter options                   ^
          --filter radiance                  ^
          --srcFaceSize 1024                 ^
          --excludeBase false                ^
          --mipCount 9                       ^
          --glossScale 10                    ^
          --glossBias 1                      ^
          --lightingModel blinnbrdf          ^
          --edgeFixup none                   ^
          --dstFaceSize 1024                 ^
          ::Processing devices               ^
          --numCpuProcessingThreads 4        ^
          --useOpenCL true                   ^
          --clVendor amd                     ^
          --deviceIndex 1                    ^
          --deviceType gpu                   ^
          ::Aditional operations             ^
          --inputGammaNumerator 2.2          ^
          --inputGammaDenominator 1.0        ^
          --outputGammaNumerator 1.0         ^
          --outputGammaDenominator 2.2       ^
          --generateMipChain true            ^
          ::Output                           ^
          --outputNum 1                      ^
          --output0 "violentdays_pmrem"      ^
          --output0params dds,bgra8,cubemap
