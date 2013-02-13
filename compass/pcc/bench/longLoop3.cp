"* throws none
 * prints 500000500000
 *"
needle 2007;
"** Tests adding a lot of numbers on a range, old-fashioned way *"
"package test::range::longLoop;"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    @acc := 0;
    @i := 0;
    { @i < 1000001 } -> {
	@acc := @acc + @i;
	@i := @i + 1;
    };
    out printLine: @acc
}
