needle 2007;

"TODO doc"
"package compass::collection::size;"

"**
 * Ask the number of elements contained in me.
 *
 * @return[immutable] $Integer: Number of elements.
 *"
public $Collection abstract #size;

"**
 * Determine whether I am empty.
 *
 * A collection is empty iff it contains no elements.
 *
 * @return[global] $Boolean: if it is.
 *"
public $Collection isEmpty {
    ^size=0
}
