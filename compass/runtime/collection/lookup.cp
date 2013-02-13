needle 2007;

"TODO doc"
"package compass::collection::lookup;"

public mixin $LookupCollection;
$LookupCollection mixin $Collection;

public exception $InvalidLookupException
    #onInvalidLookup:;

"**
 * Ask for a value according to an identifying metadata property, such as an
 * index or a key. This value is the most salient value identified by that
 * property - it might be an element in the collection, or a property
 * of an element in that collection.
 *
 * @param[restrict] the identifying property
 * @return[shared] the element found
 * @throws $NoSuchElementException
 * @throws $InvalidLookupException
 *"
public $LookupCollection abstract #[];

"**
 * Add an element to me with an identifying metadata property,
 * which might remove an old element with the same metadata property.
 *
 * The new element is either the value passed in, or some combination of
 * the value passed in and the identifying property.
 *
 * @param the identifying property
 * @param[reference,shared] the value passed in
 * @throws $InvalidLookupException
 *"
public $LookupCollection abstract #:[]=;

"**
 * Mutate me so I no longer contain the element identified by an argument.
 *
 * @param identifying information 
 * @throws $InvalidLookupException
 *"
public $LookupCollection optional #delete:;

"TODO looping stuff may/will be obsolete after iterators are implemented"
"**
 * Run a block for each element in me, one at a time.
 *
 * @param[7,ro restrict] $Block: block to run
 *     @blockparam[reference] the element
 *     @blockparam[reference] the lookup
 *     @blockreturn[ignored]
 *"
$LookupCollection abstract #serialLoop:;

"**
 * Run a block for each element in me.
 *
 * @param[7,ro restrict] $Block: block to run
 *     @blockparam[reference] the element
 *     @blockparam[reference] the lookup
 *     @blockreturn[ignored]
 *"
$LookupCollection alias #-> #serialLoop:;

private $LookupCollection convertLookupFoldBlock: @blk {
    ^{ @acc, @lookup --
	@blk(@acc,[@lookup],@lookup)
    }
}

public $LookupCollection abstract #indices;
public $LookupCollection reduce: @blk withIdentity: @reduction {
    ^reduceWithLookup: @blk
	withIdentity: @reduction
	withLoopBlock: #serialLoop:;
}

public $LookupCollection reduce: @blk {
    ^indices
	singleReduce: (convertLookupFoldBlock: @blk)
	withIdentityFilter: { @index --
	    [@index]
	}
}

public $LookupCollection reduceWithLookup: @blk
			    withIdentity: @reduction
			    withLoopBlock: @loop {
    ^indices
	reduce: (convertLookupFoldBlock: @blk)
	withIdentity: @reduction
	withLoopBlock: @loop;
}

private $true thenAdd: @item to: @acc {
    ^@acc adding: @item
}
private $false thenAdd: _ to: @acc {
    ^@acc
}
public $LookupCollection filter: @filter {
    "TODO Filter type when it is before 1am @filter := @filter asFilter;"
    ^reduce: { @acc, @item, @lookup --
	@filter(@item, @lookup) thenAdd: @item to: @acc
    } withIdentity: species new
}
public $LookupCollection map: @mapping {
    ^reduce: { @acc, @item, @lookup -- 
	@acc adding: @mapping(@item, @lookup)
    } withIdentity: species new
}
