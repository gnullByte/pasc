program example(input, output);
var x : integer; var y :  integer;
function gcd(a: integer ; b : integer ): integer;
begin
   if b = 0 then gcd := a
   else gcd := gcd(b, a mod b)
end;
begin
   z := read(x, y);
   z := write(gcd(x, y));
end.
