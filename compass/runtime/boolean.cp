needle 2007;

"**
 * Basic boolean (true and false) objects.
 *
 * These objects form the foundation of logic and conditional execution
 * offering both logical operations like <@link=#and:> and conditional
 * operations like <@link=#?//>.
 *
 * @author Daniel Perelman (dap56@cornell.edu)
 * @since needle 2007
 *"
"package compass::booolean;"

"**
 * Contains common boolean messages. Creating $Boolean objects other than the
 * $true and $false singletons is not recommended.
 *
 * Custom implementations of this mixin are discouraged.
 *
 * @mutability[immutable]
 * @see $true
 * @see $false
 *"
public mixin $Boolean;

"**
 * The <@link=$Boolean> value of a true expression.
 *
 * @see $false
 *"
public singleton $true;
$true mixin $Boolean;

"**
 * The <@link=$Boolean> value of a false expression.
 *
 * @see $true
 *"
public singleton $false;
$false mixin $Boolean;

"**
 * Find the opposite of me.
 *
 * @receiver[7,ro] value to negate
 * @return[global] $Boolean
 *"
public $Boolean abstract #not;

$true not {
    ^$false
}
$false not {
    ^$true
}

"**
 * Compute non-short-circuiting logical AND. This is $true if and
 * only if both operands are $true. The second operand is a value,
 * not a block, so it is unconditionally evaluated. Use <@link=#&&>
 * if this is not what you want.
 *
 * If the first operand is false, then $false is returned.
 * If the first operand is true, then the second operand is returned.
 *
 * @receiver[7,ro] left operand
 * @param[7,ro restrict] right operand;
 * @return[param] result of generalized logical AND of the two arguments
 * @see #&&
 *"
public $Boolean abstract #&;

$true & @other {
    ^@other
}
$false & _ {
    ^$false
}

"TODO if it could be more than one of param,global,user then put each in"
"**
 * Compute short-circuiting logical AND. This is $true if and
 * only if both operands are $true. The second operand will only be evaluated
 * if the first argument is true.
 *
 * If the first operand is false, then $false is returned.
 * If the first operand is true, then the result of calling <@link=#()>
 * on the second operand is returned.
 *
 * @receiver[7,ro] left operand
 * @param[7,ro restrict] $Block: right operand;
 * @return[user] result of generalized logical AND of the two arguments
 * @see #&
 *"
public $Boolean abstract #&&;
public alias #and: #&&;

$true #&& #EvaluateFirstArgument;
$false #&& #ReturnFalse; "_ {
    ^$false
}"

"**
 * Compute non-short-circuiting logical OR. This is $false if and
 * only if both operands are $false. The second operand is a value,
 * not a block, so it is unconditionally evaluated. Use <@link=#||>
 * if this is not what you want.
 *
 * If the first operand is true, then $true is returned.
 * If the first operand is false, then the second operand is returned.
 *
 * @receiver[7,ro] left operand
 * @param[7,ro restrict] right operand;
 * @return[param] result of generalized logical OR of the two arguments
 * @see #||
 *"
public $Boolean abstract #|;

$true | _ {
    ^$true
}
$false | @other {
    ^@other
}

"**
 * Compute short-circuiting logical OR. This is $false if and
 * only if both operands are $false. The second operand will only be evaluated
 * if the first argument is false.
 *
 * If the first operand is true, then $true is returned.
 * If the first operand is false, then the result of calling <@link=#()>
 * on the second operand is returned.
 *
 * @receiver[7,ro] left operand
 * @param[7,ro restrict] $Block: right operand;
 * @return[user] result of generalized logical OR of the two arguments
 * @see #|
 *"
public $Boolean abstract #||;
public alias #or: #||;

"TODO: the optimizer should do this for us!"
$true #|| #ReturnTrue; "{ 
    ^$true
}"

$false #|| #EvaluateFirstArgument; "{
    ^@arg()
}"

"**
 * Conditional evaluation. This is Compass's closest equivalent to the if/else
 * statement found in many languages.
 *
 * This acts very similarly to the ternary operator found in C-like languages
 * with the important exception that the two paths must be in blocks or they
 * will be unconditionally executed and one will be chosen to have the result
 * of calling <@link=#()> on it returned.
 *
 * Like that operator, the operator can be read as a question:
 * <code>@foo isBoolean?{@foo doSomething}//{@foo doSomethingElse}</code>
 * can be read as <q>If @foo is boolean then tell @foo to doSomething, else
 * tell @foo to doSomethingElse.</q>
 *
 * @receiver[7,ro] decides which block to execute
 * @param[7,ro restrict] $Block: evaluated if I am true
 * @param[7,ro restrict] $Block: evaluated if I am false
 * @return[user,only] result of evaluating the chosen block
 *"
public $Boolean abstract #?//;

$true #?// #EvaluateFirstArgument;
$false #?// #EvaluateSecondArgument;

"TODO: the optimizer should be able to handle these things!"
"$true ? @trueBlock // _ {
    ^@trueBlock();
}
$false ? _ // @falseBlock {
    ^@falseBlock();
}"
