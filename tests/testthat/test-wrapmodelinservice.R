context("test-wrapmodelinservice.R")

MockBmi <- R6Class(
  inherit = AbstractBmi,
  public = list(
    getComponentName = function() return('mock')
  )
)

test_that("getComponentName", {
  model = MockBmi$new()
  impl = wrapModelinService(model)
  
  result = impl$getComponentName$f()
  expect_equal(result$name, 'mock')
})
