needle 2007;

"TODO doc"
"TODO fill in messages"
"package compass::collection::ordered;"

public mixin $OrderedCollection;
$OrderedCollection mixin $LookupCollection;

"** An out of bounds index is used (negative or past end). *"
public exception $IndexOutOfBoundsException ($InvalidLookupException)
    #onIndexOutOfBounds:;

"**
 * Checks whether an index is valid for me.
 *
 * @param[7,ro restrict] $Integer: the index
 * @return[global] $Boolean: if it is
 *"
public $OrderedCollection hasIndex: @index {
    ^@index between: 0 and: (size-1)
}
"**
 * Checks whether an index is valid for me.
 *
 * @param[7,ro restrict] $Integer: the index
 * @throws $IndexOutOfBoundsException: if it is not
 *"
public $OrderedCollection checkIndex: @index {
    $IndexOutOfBoundsException assert: (hasIndex: @index)
}

"**
 * Ask for the item with the least ordering.
 *
 * @return the first item
 * @throws $IndexOutOfBoundsException: if empty
 *"
public $OrderedCollection first {
    ^[0]
}
"**
 * Ask for the item with the greatest ordering.
 *
 * @return the last item
 * @throws $IndexOutOfBoundsException: if empty
 *"
public $OrderedCollection last {
    ^[size-1]
}

"**
 * TODO
 *"
public $OrderedCollection indexRange {
    ^firstIndex...lastIndex
}
public $OrderedCollection forwardsLoop: @body {
    indexRange -> { @i --
	@body([@i], @i)
    }
}
$OrderedCollection alias #indices #indexRange;
public $OrderedCollection alias #serialLoop: #forwardsLoop:;

"**
 * TODO
 *"
public $OrderedCollection step: @step {
    @range := indexRange step: @step;
    ^|-> { _, @i -- @i in: @range }
}

"TODO doc these four when awake enough to do so"
public $OrderedCollection firstIndex {
    ^0
}
public $OrderedCollection lastIndex {
    ^size+firstIndex-1
}

public $OrderedCollection deleteFirst {
    delete: firstIndex
}
public $OrderedCollection deleteLast {
    delete: lastIndex
}

public $OrderedCollection foldl: @blk withIdentity: @id {
    ^reduceWithLookup: @blk
	withIdentity: @id
	withLoopBlock: #forwardsLoop:;
}

public $OrderedCollection foldr: @blk withIdentity: @id {
    ^reduceWithLookup: @blk
	withIdentity: @id
	withLoopBlock: #backwardsLoop:;
}

public $OrderedCollection backwardsLoop: @blk {
    indexRange backwardsLoop: { @index --
	@blk([@index],@index,)
    }
}

public $OrderedCollection reversed {
    foldr: { @acc, @item --
	@acc := @acc addingFirst: @item
    } withIdentity: species new
}

public $OrderedCollection addingLastAll: @other {
    ^@other foldl: { @acc, @item --
	@acc addingLast: @item
    } withIdentity: self
}

public $OrderedCollection addingFirstAll: @other {
    ^@other foldr: { @acc, @item --
	@acc addingFirst: @item
    } withIdentity: self
}

public $OrderedCollection [ @start ~ @end ~ @step ] {
    "TODO: implement this based off of some other method"
    @range :=
	(@start default: { firstIndex })...
	    (@end default: { lastIndex}) step: (@step default: {1});
    ^@out := species new;
    @range forwardsLoop: { @oldIndex, @newIndex --
	@out add: [@oldIndex]
    }
}

public exception $NoMatchException #onNoMatch:;
public $OrderedCollection indexOfElementMatching: @blk {
    forwardsLoop: { @elem, @index --
	@blk(@elem) and: {^@index}
    };
    $NoMatchException()
}

public $OrderedCollection indexOfElementMatching: @blk
					    from: @start
					    to:   @finish {
    @start...@finish forwardsLoop: { @index --
	@blk([@index]) and: {^@index}
    };
    $NoMatchException()
}

public $OrderedCollection indexOfLastElementMatching: @blk {
    {
	^@index := (indexOfElementMatching: { @item --
	    @blk(@item) not
	}) - 1;
    } onNoMatch: {
	^size - 1
    }
}

public $OrderedCollection allInitialElementsMatching: @blk {
    ^[firstIndex ~ indexOfLastElementMatching: @blk ~ 1]
}
