package com.hiperbou.tiled

@OptIn(ExperimentalStdlibApi::class)
fun TileMapLayer.toHeader(layerName:String):String {
    val NUM_COLS_SIZE = 2
    val NUM_ROWS_SIZE = 2
    val TILE_SIZE = 1

    val HEADER_SIZE = NUM_COLS_SIZE + NUM_ROWS_SIZE

    fun Int.toHex() = "0x" + ("%x".format(this).padStart(2, '0'))
    fun Int.to16bitHex():String {
        val lv = this shr 8
        val rv = this and 0x00ff
        return "${lv.toHex()}, ${rv.toHex()}"
    }


    return """
        static const unsigned int ${layerName}_size=${HEADER_SIZE + (numCols*numRows*TILE_SIZE)};
        static const unsigned char $layerName[] = {
            ${numCols.to16bitHex()}, 
            ${numRows.to16bitHex()},
            ${map.joinToString { it.toHex() }}
        };
    """.trimIndent()

}