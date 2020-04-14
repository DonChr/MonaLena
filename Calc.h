/***********************************************************************
*
* Copyright (c) 2020 Dr. Chrilly Donninger
* The code can be freely used for private and educational projects
* Commerical users must ask the author for permission at c.donninger@wavenet.at
*
* This file is part of MonaLisa
*
***********************************************************************/
#pragma once
#include <cstdint>
/**
<summary>A collection of simple helper functions</summary>
*/

/**
<summary>Calculates the Median of 5 values as a comparator network</summary>
*/
   int32_t Median5(int32_t v0, int32_t v1, int32_t v2, int32_t v3, int32_t v4);
/**
<summary>Calculates the Median of 9 values as a comparator network</summary>
*/
int32_t Median9(int32_t v0, int32_t v1, int32_t v2, int32_t v3, int32_t v4, int32_t v5, int32_t v6, int32_t v7, int32_t v8);
 

