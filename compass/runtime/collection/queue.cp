needle 2007;

"TODO doc"
"TODO should [de]queue be general collection methods?"
"package compass::collection::queue;"

public mixin $Queue;
$Queue mixin $Collection;

"**
 * Remove my last element. The last element is the element with the greatest
 * ordering. For arbitrary ordering systems, this means the highest index.
 * For sorted collections, this means the item that sorts the highest.
 * TODO: Segregate this doc out into more specific documentations.
 *
 * @return[reference] my former last element
 *"
public $Queue abstract #dequeue;

"**
 * Add a new element as my first element, or,
 * if the collection is self-sorting, the first element among
 * all elements sorted as equivalent to it.
 *
 * @param[reference] the new first element
 * @see #push:
 *" 
public $Queue abstract #enqueue:;
