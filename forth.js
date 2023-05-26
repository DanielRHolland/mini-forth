function forth(print = console.log) {
    const s = []; // the stack
    const popNum = () => Number(s.pop());

    const fs = {
        '/' : () => {
                const a2 = popNum();
                const a1 = popNum();
                s.push(a1 / a2);
            },
        '-' : () => {
                const a2 = popNum();
                const a1 = popNum();
                s.push(a1 - a2);
            },
        '+' : () => { s.push(popNum() + popNum()) },
        '*' : () => { s.push(popNum() * popNum()) },
        '=' : () => { s.push(popNum() === popNum()) },
        'swap' : () => {
          const a1 = s.pop();
          const a2 = s.pop();
          s.push(a1);
          s.push(a2);
        },
        'drop' : () => {
          s.pop();
        },
        'over' : () => {
          const a1 = s.pop();
          const a2 = s.pop();
          s.push(a2);
          s.push(a1);
          s.push(a2);
        },
        'rot' : () => {
          const a1 = s.pop();
          const a2 = s.pop();
          const a3 = s.pop();
          s.push(a2);
          s.push(a1);
          s.push(a3);
        },
        'then' : () => {/*Doing nothing skips the token*/},
        'if' : (initialIdx, tokens) => {
          if (!s.pop()) {
            let localIdx = initialIdx;
            while (tokens[localIdx] && tokens[localIdx] !== 'then') {localIdx++}
            return localIdx - initialIdx;
          }
        },
        'not' : () => { s.push(!s.pop()) },
        '.' : () => { print(s.pop()) },
        'peek' : () => { print(s[s.length - 1]) },
        'dup' : () => {
          const a = s.pop();
          s.push(a);
          s.push(a);
        },
        ':' : (initialIdx, tokens) => {
            let localIdx = initialIdx + 1;
            const fname = tokens[localIdx++];
            const fnContent = [];
            for (;tokens[localIdx] !== ';';  localIdx++) {
                fnContent.push(tokens[localIdx]);
            }
            fs[fname] = () => {
              exec(fnContent);
            }
            return localIdx - initialIdx; // numItemsSkipped
        },
    }

    function exec(tokens) {
        for (let tokenIdx = 0; tokenIdx < tokens.length; tokenIdx++) {
          const token = tokens[tokenIdx];
          if (fs[token]) {
            const numItemsSkipped = fs[token](tokenIdx, tokens);
            if (Number.isInteger(numItemsSkipped)) {
              tokenIdx += numItemsSkipped;
            }
          }
          else {
            s.push(token);
          }
        }
    }
    return t =>
      new Promise(
        resolve => {
          exec(t.split(/[ \n\t]+/).filter(x => x !== ''));
          resolve();
        }
      );
}

/*
root tag to add to page:
<div id="forthroot"></div>

 * */

if (typeof window !== 'undefined') { // browser UI
  const style = document.createElement('style')
   style.innerHTML = `
      #forthroot * {
        background-color: #42456f;
        color: white;
	font-family: monospace;
	font-size: 1.2rem;
        box-sizing: border-box;
      }
      #forthouter {
        cursor: text;
        border: solid thin black;
      }
      #forthline {
        width: 97%;
        border: none;
      }
      #forthline:focus {
        outline: none;
      }
      #forthresultsbox {
        height: 90vh;
      }
      #forthresults {
        height: 100%;
        margin-top: 0px;
        overflow: clip;
        margin: 0px;
      }
      #forthlinelabel {
        padding-top:1px;
        max-width: 3%;
      }
      #forthform {
        margin-bottom: 0px;
      }
  `;
  document.head.appendChild(style);
  forthroot.innerHTML = `
    <div id="forthouter">
      <form id="forthform">
        <label id="forthlinelabel" for="forthline" position="left">></label>
        <input id="forthline" type="text" value="" autocomplete="off">
      </form>
      <div id='forthresultsbox'>
        <pre id="forthresults"></pre>
      </div>
    </div>`
  forthouter.onclick = () => forthline.focus();
  const print = x => { forthresults.innerHTML = x + '\n' + forthresults.innerHTML; };
  const m = forth(print);
  forthform.onsubmit = () => {
    const input = forthline.value;
    if (input !== '') {
      m(input).then(() => print(`> ${input}`));
      forthline.value = '';
    }
    return false;
  }

} else { // UI if NodeJS
  const readline = require('readline');
  const rl = readline.createInterface({ input: process.stdin, output: process.stderr });
  const prompt = (query) => new Promise((resolve) => rl.question(query, resolve));
  const m = forth();
  (async () => {
    while (true) {
      const input = await prompt("> ");
      m(input)
    }
  })();
}
