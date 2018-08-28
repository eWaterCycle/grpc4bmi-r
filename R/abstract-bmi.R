library(R6)

#' Abstract class defining the basic model interface
#' 
#' Documentation on each function can be found at https://github.com/csdms/bmi/tree/master/docs/source
#'
#' @export
#'
AbstractBmi <- R6Class(
  public = list(
    # R6 constructor is also called initialize so rename bmi initialize
    bmi_initialize = function(config_file) stop('Not implemented'),
    update = function() stop('Not implemented'),
    updateUntil = function(until) stop('Not implemented'),
    updateFrac = function(frac) stop('Not implemented'),
    # R6 destructor is also called finalize so rename bmi finalize
    bmi_finalize = function() stop('Not implemented'),
    runModel = function() stop('Not implemented'),

    getComponentName = function() stop('Not implemented'),
    getInputVarNames = function() stop('Not implemented'),
    getOutputVarNames = function() stop('Not implemented'),

    getTimeUnits = function() stop('Not implemented'),
    getTimeStep = function() stop('Not implemented'),
    getCurrentTime = function() stop('Not implemented'),
    getStartTime = function() stop('Not implemented'),
    getEndTime = function() stop('Not implemented'),

    getVarGrid = function(name) stop('Not implemented'),
    getVarType = function(name) stop('Not implemented'),
    getVarItemSize = function(name) stop('Not implemented'),
    getVarUnits = function(name) stop('Not implemented'),
    getVarNBytes = function(name) stop('Not implemented'),

    getValue = function(name) stop('Not implemented'),
    getValueAtIndices = function(name, indices) stop('Not implemented'),

    setValue = function(name, values) stop('Not implemented'),
    setValueAtIndices = function(name, indices, values) stop('Not implemented'),

    getGridSize = function(grid_id) stop('Not implemented'),
    getGridType = function(grid_id) stop('Not implemented'),
    getGridRank = function(grid_id) stop('Not implemented'),
    getGridShape = function(grid_id) stop('Not implemented'),
    getGridSpacing = function(grid_id) stop('Not implemented'),
    getGridOrigin = function(grid_id) stop('Not implemented'),
    getGridX = function(grid_id) stop('Not implemented'),
    getGridY = function(grid_id) stop('Not implemented'),
    getGridZ = function(grid_id) stop('Not implemented'),
    getGridConnectivity = function(grid_id) stop('Not implemented'),
    getGridOffset = function(grid_id) stop('Not implemented')
  )
)