// Copyright 2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use crate::{
    advertisement::ModelId,
    fetcher::{DeviceInfo, FpFetcher},
};

/// A struct for mocking retrieval of Fast Pair data.
pub(crate) struct FpFetcherMock {
    get_device_info_from_model_id: Result<DeviceInfo, anyhow::Error>,
}

impl FpFetcherMock {
    pub(crate) fn new(get_device_info_from_model_id: Result<DeviceInfo, anyhow::Error>) -> Self {
        FpFetcherMock {
            get_device_info_from_model_id,
        }
    }
}

impl FpFetcher for FpFetcherMock {
    fn get_device_info_from_model_id(
        &self,
        _model_id: &ModelId,
    ) -> Result<DeviceInfo, anyhow::Error> {
        match &self.get_device_info_from_model_id {
            Ok(result) => Ok(result.clone()),
            Err(_) => Err(anyhow::anyhow!("intentional mock error")),
        }
    }
}
