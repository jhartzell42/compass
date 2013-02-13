"* throws none
 * prints 5000050000
 *"
needle 2007;
"** Tests adding a lot of numbers on a range, old-fashioned way *"
"package test::range::longLoop;"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    @acc := 0;
    1...100000 -> { @val -- @acc := @acc + @val };
    out printLine: @acc
}
