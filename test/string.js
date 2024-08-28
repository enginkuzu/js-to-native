
let text = "Hello world!";

console.log(text);

console.log(text.charAt(0));
console.log(text.charAt(11));
console.log(text.charAt(12));
//console.log(text.charAt(-12));
//console.log(text.charAt(-13));

console.log(text.charCodeAt(0));
console.log(text.charCodeAt(11));
console.log(text.charCodeAt(12));
//console.log(text.charCodeAt(-12));
//console.log(text.charCodeAt(-13));

let utf16 = "a܍☺܍a";
console.log(utf16.charAt(3));
console.log(utf16.charCodeAt(3));
console.log(utf16.substr(3, 1));
