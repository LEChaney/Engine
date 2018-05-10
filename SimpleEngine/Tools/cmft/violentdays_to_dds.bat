cmft   %* --inputFacePosX violentdays_rt.tga ^
          --inputFaceNegX violentdays_lf.tga ^
          --inputFacePosY violentdays_up.tga ^
          --inputFaceNegY violentdays_dn.tga ^
          --inputFacePosZ violentdays_bk.tga ^
          --inputFaceNegZ violentdays_ft.tga ^
          ::Filter options                   ^
          --filter none                      ^
          --srcFaceSize 1024                 ^
          --dstFaceSize 1024                 ^
          ::Output                           ^
          --outputNum 1                      ^
          --output0 "violentdays"            ^
          --output0params dds,bgra8,cubemap
