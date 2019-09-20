var Module = {
  onRuntimeInitialized: () => init(Module)
};

function init() {
    console.log(window.Module.asurionadd(50,60))
}
