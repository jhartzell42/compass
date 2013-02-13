needle 2007;

"**
 * Simple default arguments support.
 *
 * Pass $default as a message argument in order to specify the default value
 * for that parameter. Methods can accept default values easily:
 * <code>@arg := @arg default: { $argDefaultValue };</code>
 *
 * Use of $default for any other purpose is strongly discouraged because
 * it would likely lead to a situation where one wants to pass the value
 * $default into a message instead of using it to mark a default value.
 *"
"package compass::default;"

"**
 * The value passed to specify a default should be used.
 *"
public singleton $default;

"**
 * Finds the proper value of a method argument.
 *
 * If I am any object other than
 * $default then this simply returns myself. If I am $default, then
 * the block is evaluated and its value returned.
 *
 * @receiver[7,ro] the value passed into the method
 * @param[7,ro restrict] $Block: block which evaluates to the default value
 * @return[user] the value to use for the method
 *"
public #default:;

$Object default: _;
$default default: @blk {
    ^@blk()
}
