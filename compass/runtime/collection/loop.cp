needle 2007;

"TODO doc"
"package compass::collection::loop;"

"**
 * Run a block for each element in me, one at a time.
 *
 * @param[7,ro restrict] $Block: block to run
 *     @blockparam[reference] the element
 *     @blockreturn[ignored]
 *"
$Collection abstract #serialLoop:;

"**
 * Run a block for each element in me.
 *
 * @param[7,ro restrict] $Block: block to run
 *     @blockparam[reference] the element
 *     @blockreturn[ignored]
 *"
$Collection alias #-> #serialLoop:;
