import com.hiperbou.tiled.*
import com.hiperbou.tiled.parseAs

fun main(args: Array<String>) {
    if(args.isEmpty()) {
        println("Usage: tmx2h filename")
        return
    }

    val filename = args[0].removeSuffix(".tmx")
    val variableName = filename.substringAfterLast("/").replace("-","_")

    val tiledTileMapXML = parseAs<TiledTileMapXML>("$filename.tmx")


    val TILE_MASK_SOLID = 0x80           //1000 0000
    val TILE_MASK_ONE_WAY = 0x40         //0100 0000
    val TILE_MASK_BREAKABLE = 0x20       //0010 0000
    val TILE_MASK_FLOOR = 0xC0           //1100 0000
    val TILE_MASK_SOLID_BREAKABLE = 0xA0 //1010 0000
    val collisionMapper = TileMapper(mapOf(
        1 to TILE_MASK_SOLID_BREAKABLE,
        3 to TILE_MASK_ONE_WAY
    ))

    val defaultMapper = TileMapper(mapOf())

    val mappers = mapOf("collision" to collisionMapper)

    val layers = tiledTileMapXML.layers.map {
        val mapper = mappers.getOrElse(it.name!!) { defaultMapper }
        TileMapLayer.fromTiledTileMapLayer(it, mapper)
    }
    layers.forEach {
        println(it.toHeader(variableName + "_" + it.name))
    }
}