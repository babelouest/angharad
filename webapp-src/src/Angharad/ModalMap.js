import React, { Component } from 'react';
import i18next from 'i18next';

class ModalMap extends Component {
  constructor(props) {
    super(props);

    this.state = {
      profileList: props.profileList,
      map: props.map,
      add: props.add,
      cb: props.cb,
      nameValid: true,
      nameError: false,
      imageError: false
    }

    this.closeModal = this.closeModal.bind(this);
    this.changeName = this.changeName.bind(this);
    this.changeDescription = this.changeDescription.bind(this);
    this.onUploadImage = this.onUploadImage.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

  changeName(e) {
    let map = this.state.map;
    map.name = e.target.value;
    let isValid = true, nameValid = true, nameError = false;
    if (!map.name) {
      nameValid = false;
    } else {
      this.state.profileList.forEach((curProfile) => {
        if (curProfile.name === map.name) {
          isValid = false;
          nameValid = false;
          nameError = i18next.t("maps.name-taken");
        }
      });
    }
    this.setState({map: map, isValid: isValid, nameValid: nameValid, nameError: nameError});
  }
  
  changeDescription(e) {
    let map = this.state.map;
    map.description = e.target.value;
    this.setState({map: map});
  }
  
  uploadImage() {
    $('#uploadImageInput').trigger('click');
  }

  onUploadImage(event) {
    var fileList = event.target.files;
    if (fileList.length > 0) {
      var file = fileList[0];
      if (file.size > (8 * 1024 * 1024)) {
        this.setState({imageError: true});
      } else {
        var fr = new FileReader();
        fr.onload = (ev) => {
          var map = this.state.map;
          map.image = ev.target.result;
          this.setState({map: map, imageError: false});
        };

        fr.readAsDataURL(file);
      }
    }
  }

  closeModal(e, result) {
    e.preventDefault();
    if (this.state.cb) {
      this.state.cb(result, this.state.map);
    }
  }
  
	render() {
    let requiredStar, errorMessageJsx, errorClass = "";
    if (this.state.add) {
      requiredStar = <span className="text-danger"> *</span>;
      if (!this.state.nameValid) {
        if (this.state.nameError) {
          errorMessageJsx = <span className="text-danger"> {this.state.nameError}</span>
        }
        errorClass = " is-invalid";
      }
    }
    let thumbJsx;
    if (this.state.map.image) {
      thumbJsx = <img src={this.state.map.image} width="40" height="40" />
    }
    let errorImageJsx;
    if (this.state.imageError) {
      errorImageJsx = <span className="text-danger"> {i18next.t("maps.image-error")}</span>
    }
		return (
      <div className="modal" tabIndex="-1" id="modalMap">
        <div className="modal-dialog">
          <div className="modal-content">
            <form onSubmit={(e) => this.closeModal(e, true)}>
              <div className="modal-header">
                <h5 className="modal-title">
                  {i18next.t("maps.map-editor")}
                </h5>
                <button type="button" className="btn-close" aria-label="Close" onClick={this.closeModal}></button>
              </div>
              <div className="modal-body">
                <div className="form-floating mb-3">
                  <input type="text" className={"form-control" + errorClass} id="mapName" value={this.state.map.name} disabled={!this.state.add} onChange={this.changeName}/>
                  <label htmlFor="mapName">
                    {i18next.t("maps.modal-name")}
                    {requiredStar}
                  </label>
                  {errorMessageJsx}
                </div>
                <div className="form-floating mb-3">
                  <input type="text" className="form-control" id="mapDescription" value={this.state.map.description} onChange={this.changeDescription}/>
                  <label htmlFor="mapDescription">
                    {i18next.t("maps.modal-description")}
                  </label>
                </div>
                <div className="mb-3">
                  <input type="file"
                         id="uploadImageInput"
                         onChange={this.onUploadImage}
                         accept=".jpg,.jpg,.png,.gif,.svg"
                         className="upload"/>
                  <button type="button" className="btn btn-secondary" onClick={this.uploadImage}>{i18next.t("maps.modal-image")}</button>
                  <div className="elt-bottom">
                    {errorImageJsx}
                  </div>
                  <div className="elt-bottom">
                    {thumbJsx}
                  </div>
                </div>
              </div>
              <div className="modal-footer">
                <button type="submit" className="btn btn-secondary" onClick={(e) => this.closeModal(e, false)}>{i18next.t("modal.close")}</button>
                <button type="button" className="btn btn-primary" onClick={(e) => this.closeModal(e, true)} disabled={!this.state.map.name || !this.state.map.image || this.state.imageError || !this.state.nameValid}>{i18next.t("modal.ok")}</button>
              </div>
            </form>
          </div>
        </div>
      </div>
		);
	}
}

export default ModalMap;
