if (window.script1Loaded) {
    window.script2Loaded = true;
    const el = document.createElement('div');
    el.id = 'scriptloader-success';
    el.textContent = 'ScriptLoader works!';
    document.body.appendChild(el);
}
