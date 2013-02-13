needle 2007;

"TODO doc"
"TODO What exactly should be (0/1,ro) vs. fully accessible?"
"package compass::collection::map!!hash;"

"** Very simple hash map implementation. *"
public class $HashMap($PrimitiveArrayWrapper)
    !hashFunction,
;
"** An empty space in a hash map. *"
"TODO how do I let subclasses define behavior for empty spaces without letting
    just anyone use $emptySpace in hash maps -- which would break them."
private singleton $emptySpace;

public $HashMap constructor
	newWithCapacity: @capacity hashFunction: !hashFunction {
    super newWithCapacity: @capacity;
    array fillWith: $emptySpace;
}
public $HashMap constructor newWithCapacity: @capacity {
    newWithCapacity: @capacity hashFunction: $defaultHashFunction
}
public $HashMap constructor newWithHashFunction: @hashFunc {
    newWithCapacity: class defaultCapacity hashFunction: @hashFunc
}

private global $defaultCapacity = 4;
$HashMap class defaultCapacity {
    ^$defaultCapacity
}
private global $defaultLoadFactor = 3/4;
$HashMap class defaultLoadFactor {
    ^$defaultLoadFactor
}
public $HashMap loadFactor {
    ^class defaultLoadFactor
}
public $HashMap emptySpace (0,ro) {
    ^$emptySpace
}
public $HashMap arraySizeForCapacity: @capacity (1,ro) {
    ^(@capacity / loadFactor) ceil
}
public $HashMap hashFunction (1,ro) {
    ^!hashFunction
}
public $HashMap equivalenceRelation (1,ro) {
    ^hashFunction
}
public $HashMap hash: @key (1,ro) {
    ^hashFunction hash: @key below: array size
}
public $HashMap abstract #indexOf:;
