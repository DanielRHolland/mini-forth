function forth(print = console.log) {
    const s = []; // the stack
    const popNum = () => Number(s.pop());

    const fs = {
        ' ' : () => {},
        '\n' : () => {},
        '\t' : () => {},
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
        '.' : () => print(s.pop()),
        'peek' : () => print(s[s.length - 1]),
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
        's' : () => print(JSON.stringify(s)),
        'sum' : () => {
          let total = 0;
          while (s.length > 0) {
            total += popNum();
          }
          s.push(total);
        },
        '(' : (initialIdx, tokens) => {
          let localIdx = initialIdx + 1;
          while (tokens[localIdx] !== ')') { localIdx++ }
          return localIdx - initialIdx;
        }
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
    return t => exec(t.split(/[ \n\t]+/));
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
        margin-top: 0px;
      }
      #forthlinelabel {
        padding-top:1px;
        max-width: 3%;
      }
  `;
  document.head.appendChild(style);
  forthroot.innerHTML = `
    <div id="forthouter">
      <form id="forthform">
        <label id="forthlinelabel" for="forthline" position="left">></label>
        <input id="forthline" type="text" value="">
      </form>
      <div id='forthresultsbox'>
        <pre id="forthresults"></pre>
      </div>
    </div>`
  forthouter.onclick = () => forthline.focus();
  const print = x => { forthresults.innerHTML = x + '\n' + forthresults.innerHTML; };
  const m = forth(print);
  forthform.onsubmit = () => {
    print(`> ${forthline.value}`);
    if (forthline.value !== '') {
      m(forthline.value);
      forthline.value = '';
    }
    return false;
  }

} else { // UI if NodeJS
  const readline = require('readline');
  const rl = readline.createInterface({ input: process.stdin, output: process.stdout });
  const prompt = (query) => new Promise((resolve) => rl.question(query, resolve));
  const m = forth();
  (async () => {
    while (true) {
      const input = await prompt("> ");
      m(input)
    }
  })();
}
