package com.hiperbou.tiled

class TileMapper(private val map:Map<Int, Int>) {
    fun map(gid:Int) = map.getOrElse(gid) { gid }
}
