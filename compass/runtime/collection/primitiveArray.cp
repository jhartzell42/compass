needle 2007;

"**
 * Compass side of the low-level implementation of arrays.
 *
 * @author Daniel Perelman (dap56@cornell.edu)
 * @since needle 2007
 *"
"package compass::collection!!primitiveArray"

"**
 * The class for the array language primitive. Note that the actual size of
 * the object is affected by the number of elements.
 *
 * Due to technical limitations, this class cannot be extended.
 *"
public "final" class $PrimitiveArray
    "**
     * $Integer: The number of elements I can hold.
     *"
    !size,
    "special";
$PrimitiveArray mixin $List;
$PrimitiveArray mixin $FixedSizeCollection;

"**
 * Initialize my size field.
 *"
private $PrimitiveArray initializeWithSize: !size (0,rw);

"**
 * Construct a new primitive array with the specified capacity.
 *
 * @param[7,ro saved] the number of elements the constructed array can hold
 * @return[1,rw only] the new primitive array
 *"
public $PrimitiveArray class newWithSize: @size {
    ^(basicNewWithSize: @size+1) {initializeWithSize: @size; secure}
}

"**
 * Create an array with the specified size. Note that one space is used for
 * holding the size.
 *
 * @param[7,ro restrict] $Integer: number of elements in the new array plus
				   one
 * @return[0,rw only] $PrimitiveArray: an array with the specified size
 *"
private $PrimitiveArray class #basicNewWithSize: #PrimitiveArrayCreate;

"**
 * Ask for one of my elements.
 *
 * @param[7,ro restrict] $Integer: which position, where 0 is the first
 * @param[7,ro restrict] $Block: the block to call when index is out of bounds
 * @return[user] the specified element; undefined if never set
 *"
private $PrimitiveArray #primitiveGet:onBounds: #PrimitiveArrayGet;

"**
 * Sets one of my elements.
 *
 * @param[7,ro restrict] $Integer: which position, where 0 is the first
 * @param[7,ro saved] object to store
 * @param[7,ro restrict] $Block: the block to call when index is out of bounds
 *"
private $PrimitiveArray #primitiveAt:put:onBounds: #PrimitiveArraySet;

"Actual public stuff"
$PrimitiveArray size (7,ro) {
    ^!size
}
$PrimitiveArray [ @index ] {
    ^primitiveGet: @index onBounds: {
	$IndexOutOfBoundsException()
    }
}
$PrimitiveArray :[ @index ]= @val {
    primitiveAt: @index put: @val onBounds: {
	$IndexOutOfBoundsException()
    }
}

"TODO doc below here"
public $PrimitiveArray fillWith: @obj {
    indexRange -> { @i --
	:[@i]= @obj
    }
}

$PrimitiveArray species {
    ^$ArrayList
}

"** A friendly wrapper for collections implemented using a primitive array. *"
public class $PrimitiveArrayWrapper
    !array,
    !size,
;

public $PrimitiveArrayWrapper abstract #arraySizeForCapacity:;
public $PrimitiveArrayWrapper constructor newWithCapacity: @capacity {
    "TODO right exception?"
    $InvalidArgumentException assert: @capacity isNegative not;
    !array := $PrimitiveArray newWithSize:
	(arraySizeForCapacity: @capacity);
    !size := 0;
}
public $PrimitiveArrayWrapper class abstract #defaultCapacity;
$PrimitiveArrayWrapper constructor new {
    newWithCapacity: class defaultCapacity
}

$PrimitiveArrayWrapper size (7,ro) {
    ^!size
}
public $PrimitiveArrayWrapper setSize: !size (1,rw);
public $PrimitiveArrayWrapper array (1,ro) {
    ^!array
}
public $PrimitiveArrayWrapper setArray: !array (1,rw);
