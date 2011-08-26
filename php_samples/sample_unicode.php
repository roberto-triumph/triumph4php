<html>
<head>
	<meta http-equiv="content-type" content="text/html; charset=UTF-8">
</head>
<body>
<?php

$message = <<<EOF
Typing "ん"

    * ん has some peculiarities. To type ん at the end of a word, you must type "n-n". But if you want to type a word where ん is not followed by a な gyo, you can just type one "n". e.g.: to get こんど, you would type "k-o-n-d-o". If ん is followed by a な gyo, e.g.: to get さんにん, you would have to type "s-a-n-n-n-i-n-n". If the word is followed by an あ gyo, then you must also type two "n"s, e.g.: "s-i-n-n-a-i" for しんあい. 

Typing vowel extensions

    * In order to get the long "o" sound, the user must actually type in "u", e.g.: type "k-y-o-u" in order to get きょう. For katakana, use the dash "-" for vowel extensions. 

Typing in Katakana

    * to type in katakana, simply press Shift or CapsLock, or type in hiragana and then convert to katakana with the spacebar. As you may have noticed on the kana chart, there are a few unconventional combinations of characters. These will probably be used more for foreign words, e.g: type in "f-o- - -m-u", then press the Spacebar, then Enter to accept the katakana conversion to the English word "form", which is in the program dictionary. You will however, have to type in capitals in order to get proper nouns in katakana. 

Typing Numbers

    * you may convert arabic numbers to kanji, e.g.:１００ to 百. 

THE BUFFER
There are important things to know about buffers when you are typing Japanese. If the characters are not black, you are in an active buffer and you must do something to clear the buffer. If the computer beeps at you, it means that you are trying to do something outside of the active buffer, and you must clear it (by pressing Enter) before you can continue.

When in the Roman-to-Kanji keyboard, this is the process for entering text: 
EOF;

echo $message;
?>
</body>
</html>
