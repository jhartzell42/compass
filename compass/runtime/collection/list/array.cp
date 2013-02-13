needle 2007;

"TODO doc"
"package compass::collection::list!!array"

public class $ArrayList($PrimitiveArrayWrapper);
$ArrayList mixin $List;

private global $defaultCapacity = 8;
$ArrayList class defaultCapacity {
    ^$defaultCapacity
}
$ArrayList arraySizeForCapacity: @capacity {
    ^@capacity max: class defaultCapacity
}

$ArrayList [ @index ] (7,ro) {
    checkIndex: @index;
    ^array[@index]
}
$ArrayList :[ @index ]= @val (7,rw) {
    checkIndex: @index;
    array :[@index]= @val
}

private $ArrayList adjustArraySize (7,rw) {
    @newArray := $PrimitiveArray newWithSize: 2*size;
    "TODO can this copy be better?"
    (indexRange singleRangeIntersect: array indexRange) -> { @i --
	@newArray :[@i]= array[@i]
    };
    setArray: @newArray
}
private $ArrayList increaseSizeBy: @diff (7,rw) {
    (setSize: size + @diff; size) <= array size || {
	adjustArraySize
    }
}
private $ArrayList incrementSize {
    increaseSizeBy: 1
}

$ArrayList addLast: @val (7,rw) {
    incrementSize;
    :[ size-1 ]= @val;
}
"TODO should all lists addLast by default? Or... ?"
$ArrayList alias #add: #addLast:;

private $ArrayList isContentsTooBig (7,ro) {
    array size > 2*$defaultCapacity && { array size >= 4*size }
}
private $ArrayList halveIfNeeded {
    isContentsTooBig && {
	adjustArraySize
    }
}

"TODO this might belong higher up"
$ArrayList delete: @index (7,rw) {
    @index...size-2 serialLoop: { @i -- :[@i]= [@i+1] };
    setSize: size - 1;
    halveIfNeeded;
}

"not good with threading, but won't violate safety constraints either.
 won't crash, will just throw exceptions, if you use this without guarding
 it from threads"
$ArrayList forwardsLoop: @block {
    0...size-1 -> { @index --
	@block(array[@index],@index)
    }
}

$ArrayList backwardsLoop: @block {
    0...size-1 backwardsLoop: { @index --
	@block(array[@index],@index)
    }
}
