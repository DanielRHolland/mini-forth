
function exec(x) {
  return Module.ccall(
    "extern_eval", // name of C function
    "string", // return type
    ["string"], // argument types
    [x] // arguments
  );
}

/*
root tag to add to page:
<div id="forthroot"></div>

 * */

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
        height: 90vh;
        overflow: scroll;
      }
      #forthline {
        width: 97%;
        border: none;
      }
      #forthline:focus {
        outline: none;
      }
      #forthresultsbox {
      }
      #forthresults {
        margin-top: 0px;
        overflow: clip;
        margin: 0px;
      }
      #forthlinelabel {
        padding-top:1px;
        max-width: 3%;
        float: left;
      }
      #forthform {
        margin-bottom: 0px;
      }
  `;
document.head.appendChild(style);
forthroot.innerHTML = `
    <div id="forthouter">
      <div id='forthresultsbox'>
        <pre id="forthresults"></pre>
      </div>
      <form id="forthform">
        <label id="forthlinelabel" for="forthline" position="left">>&nbsp;</label>
        <input id="forthline" type="text" value="" autocomplete="off">
      </form>
    </div>`
forthouter.onclick = () => forthline.focus();
const print = x => {
  forthresults.innerHTML = forthresults.innerHTML + x ;
  forthouter.scrollTo(0, forthouter.scrollHeight);
};
forthform.onsubmit = () => {
  const input = forthline.value;
  if (input !== '') {
    print(`> ${input}\n`);
    print(exec(input));
    forthline.value = '';
  }
  return false;
}
