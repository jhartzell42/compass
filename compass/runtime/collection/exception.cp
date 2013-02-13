needle 2007;

"TODO a general exceptions package seems weird:
 (1) There should be better places for the exceptions to go.
 (2) Exceptions should be clearly separated enough within one package.
 Because of (2) moving this back into the parent package may make sense."
"package compass::collection::exception;"

"**
 * The specified element does not exist in the collection.
 *"
public exception $NoSuchElementException #onNoSuchElement:;
