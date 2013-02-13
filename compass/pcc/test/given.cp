"* throws none
 * prints 3
 * prints 3
 *"
needle 2007;
public class $Test ($ConsoleProgram);

$Integer printIntTo: @out {
    @out printLine: self
}

$Test run (7,rw) {
    @out := out;
    3 { printIntTo: @out; @a := self; @a printIntTo: @out }
}
