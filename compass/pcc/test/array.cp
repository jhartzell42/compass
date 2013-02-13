"* throws none
 * prints 42
 *"
needle 2007;

"** Tests very basic array reading an writing. *"
"package test::array;"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    @arr := $PrimitiveArray newWithSize: 1;
    @arr primitiveAt: 0 put: 42 onBounds: { foo };
    out printLine: (@arr primitiveGet: 0 onBounds: { foo });
}
